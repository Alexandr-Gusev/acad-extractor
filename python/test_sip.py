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
        for name, value in attrs_list[i]:
            print(f"attr {name} = {value}")
        for name, value in props_list[i]:
            print(f"prop {name} = {value}")
