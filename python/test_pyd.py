from acad_extractor import select_on_screen


if __name__ == "__main__":
    print("Select AutoCAD blocks on screen")
    names = []
    attrs_list = []
    props_list = []
    handles = []
    dt = select_on_screen(names, attrs_list, props_list, handles)
    print(f"Processed {len(names)} at {dt} ms")
    for i, name in enumerate(names):
        print("-" * 70)
        print(f"{name} {handles[i]}")
        for attr_name, attr_value in attrs_list[i]:
            print(f"attr {attr_name} = {attr_value}")
        for prop_name, prop_value in props_list[i]:
            print(f"prop {prop_name} = {prop_value}")
