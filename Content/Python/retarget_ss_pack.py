"""Sword and Shield Pack -> SK_Manny 일괄 리타게팅 (UE 5.7)
실행: 에디터 Output Log를 Python 모드로 바꾼 뒤 `import retarget_ss_pack` 입력
다시 실행할 때는: `import importlib, retarget_ss_pack; importlib.reload(retarget_ss_pack)`
결과: /Game/Animation/Player/Retargeted/A_Manny_*
"""
import unreal

SOURCE_MESH = "/Game/Animation/Mixamo/Idle"
TARGET_MESH = "/Game/ROG_Modular_Assets/Common/Characters/UE5_Mannequins/Manny/SK_Manny"
INPLACE_DIR = "/Game/Animation/Mixamo/Sword_and_Shield_InPlace"
PACK_DIR = "/Game/Animation/Mixamo/Sword_and_Shield_Pack"
RIG_DIR = "/Game/Animation/Retarget"
OUT_DIR = "/Game/Animation/Player/Retargeted"
# 이동 모션 원본(제자리 버전으로 대체)과 메시 없는 Y_Bot은 제외
SKIP = ("Y_Bot", "_walk", "_run", "_strafe")


def side(l):
    p, s = ("Left", "l") if l == "L" else ("Right", "r")
    fingers = [("Thumb", "thumb", 3), ("Index", "index", 3), ("Middle", "middle", 3),
               ("Ring", "ring", 3), ("Pinky", "pinky", 3)]
    src = {f"{p}Clavicle": (f"{p}Shoulder", f"{p}Shoulder"),
           f"{p}Arm": (f"{p}Arm", f"{p}Hand"),
           f"{p}Leg": (f"{p}UpLeg", f"{p}ToeBase")}
    dst = {f"{p}Clavicle": (f"clavicle_{s}", f"clavicle_{s}"),
           f"{p}Arm": (f"upperarm_{s}", f"hand_{s}"),
           f"{p}Leg": (f"thigh_{s}", f"ball_{s}")}
    for a, b, n in fingers:
        src[f"{p}{a}"] = (f"{p}Hand{a}1", f"{p}Hand{a}{n}")
        dst[f"{p}{a}"] = (f"{b}_01_{s}", f"{b}_0{n}_{s}")
    return src, dst


SRC = {"Spine": ("Spine", "Spine2"), "Head": ("Neck", "Head")}
DST = {"Spine": ("spine_01", "spine_05"), "Head": ("neck_01", "head")}
for l in "LR":
    a, b = side(l)
    SRC.update(a)
    DST.update(b)

tools = unreal.AssetToolsHelpers.get_asset_tools()
lib = unreal.EditorAssetLibrary


def load(path):
    asset = unreal.load_asset(path)
    if asset is None:
        raise RuntimeError(f"에셋을 찾을 수 없음: {path}")
    return asset


def make_rig(name, mesh, root, chains):
    path = f"{RIG_DIR}/{name}"
    if lib.does_asset_exist(path):
        return load(path)
    rig = tools.create_asset(name, RIG_DIR, unreal.IKRigDefinition, unreal.IKRigDefinitionFactory())
    c = unreal.IKRigController.get_controller(rig)
    c.set_skeletal_mesh(mesh)
    if not c.set_retarget_root(root):
        raise RuntimeError(f"{name}: retarget root 실패 ({root})")
    for chain, (start, end) in chains.items():
        c.add_retarget_chain(chain, start, end, "None")
    made = len(c.get_retarget_chains())
    if made != len(chains):
        raise RuntimeError(f"{name}: 체인 {made}/{len(chains)}개만 생성됨. Output Log 확인")
    return rig


def make_retargeter(source_rig, target_rig, source_mesh, target_mesh):
    path = f"{RIG_DIR}/RTG_Mixamo_To_Manny"
    if lib.does_asset_exist(path):
        return load(path)
    rtg = tools.create_asset("RTG_Mixamo_To_Manny", RIG_DIR, unreal.IKRetargeter, unreal.IKRetargetFactory())
    c = unreal.IKRetargeterController.get_controller(rtg)
    src, tgt = unreal.RetargetSourceOrTarget.SOURCE, unreal.RetargetSourceOrTarget.TARGET
    c.add_default_ops()
    c.set_ik_rig(src, source_rig)
    c.set_ik_rig(tgt, target_rig)
    c.assign_ik_rig_to_all_ops(src, source_rig)
    c.assign_ik_rig_to_all_ops(tgt, target_rig)
    c.set_preview_mesh(src, source_mesh)
    c.set_preview_mesh(tgt, target_mesh)
    c.auto_map_chains(unreal.AutoMapChainType.EXACT, True)
    c.auto_align_all_bones(tgt)
    return rtg


def collect():
    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    found = []
    for folder, skip in ((INPLACE_DIR, ()), (PACK_DIR, SKIP)):
        for data in registry.get_assets_by_path(folder, recursive=False):
            name = str(data.asset_name)
            if str(data.asset_class_path.asset_name) == "AnimSequence" and not any(k in name for k in skip):
                found.append(data)
    return found


def main():
    source_mesh, target_mesh = load(SOURCE_MESH), load(TARGET_MESH)
    source_rig = make_rig("IK_Mixamo", source_mesh, "Hips", SRC)
    target_rig = make_rig("IK_Manny", target_mesh, "pelvis", DST)
    rtg = make_retargeter(source_rig, target_rig, source_mesh, target_mesh)
    assets = collect()
    if not any(INPLACE_DIR in str(a.package_name) for a in assets):
        unreal.log_warning(f"{INPLACE_DIR} 에 제자리 이동 모션이 없습니다. FBX 임포트를 먼저 확인하세요.")
    results = unreal.IKRetargetBatchOperation.duplicate_and_retarget(
        assets, source_mesh, target_mesh, rtg,
        search="sword_and_shield_", replace="", prefix="A_Manny_", suffix="",
        include_referenced_assets=False, overwrite_existing_files=True)
    moved = 0
    for data in results:
        old = str(data.package_name)
        new = f"{OUT_DIR}/{data.asset_name}"
        if old != new:
            if lib.does_asset_exist(new):
                lib.delete_asset(new)
            moved += bool(lib.rename_asset(old, new))
    lib.save_directory(RIG_DIR)
    lib.save_directory(OUT_DIR)
    unreal.log(f"[retarget_ss_pack] 입력 {len(assets)}개, 결과 {len(results)}개, 이동 {moved}개 -> {OUT_DIR}")


main()
