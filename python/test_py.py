import win32com.client
import time


def select_on_screen(names, attrs_list, props_list, handles=None, callback=None):
    try:
        app = win32com.client.Dispatch("AutoCAD.Application")
    except:
        raise RuntimeError("no app")

    if not app.Visible:
        app.Quit()
        raise RuntimeError("app.Visible == False")

    try:
        doc = app.ActiveDocument
    except:
        raise RuntimeError("no doc")

    if doc is None:
        raise RuntimeError("doc is None")

    try:
        selection = doc.ActiveSelectionSet
        selection.Clear()
        selection.SelectOnScreen()
    except:
        raise RuntimeError("selection error")

    t = time.time()
    count = len(selection)
    callback_delta = int(count / 100)
    if callback_delta == 0:
        callback_delta = 1
    for i, item in enumerate(selection):
        if callback is not None and i % callback_delta == 0:
            callback(i, count)
        if item.EntityName == "AcDbBlockReference":
            names.append(item.EffectiveName)
            if handles is not None:
                handles.append(item.Handle)

            attrs = []
            for attr in item.GetAttributes():
                attrs.append((attr.TagString, attr.TextString))
            attrs_list.append(attrs)

            props = []
            for prop in item.GetDynamicBlockProperties():
                props.append((prop.PropertyName, prop.Value))
            props_list.append(props)
    if callback is not None:
        callback(count, count)
    return int((time.time() - t) * 1000)


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
