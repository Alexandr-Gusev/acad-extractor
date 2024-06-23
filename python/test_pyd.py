from acad_extractor import select_on_screen


if __name__ == "__main__":
    print("Select AutoCAD blocks on screen")
    names = []
    attrs_list = []
    props_list = []
    handles = []
    points = []
    dt = select_on_screen(names, attrs_list, props_list, handles, None, points)
    print(f"Processed {len(names)} at {dt} ms")
    for i, name in enumerate(names):
        print("-" * 70)
        print(f"{name} {handles[i]} ({points[i][0]}, {points[i][1]}, {points[i][2]})")
        for attr_name, attr_value in attrs_list[i]:
            print(f"attr {attr_name} = {attr_value}")
        for prop_name, prop_value in props_list[i]:
            print(f"prop {prop_name} = {prop_value}")
