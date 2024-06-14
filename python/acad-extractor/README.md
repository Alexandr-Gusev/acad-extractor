# acad-extractor

A quick tool to extract block attributes and properties from AutoCAD drawings.

## Usage

```python
from acad_extractor import select_on_screen

print("Select AutoCAD blocks on screen")
names = []
attrs_list = []
props_list = []
handles = []
def callback(processed, total):
    print(f"Processed {processed}/{total}")
print("-" * 70)
dt = select_on_screen(names, attrs_list, props_list, handles, callback)
print("-" * 70)
print(f"Processed {len(names)} at {dt} ms")
for i, name in enumerate(names):
    print("-" * 70)
    print(f"{name} {handles[i]}")
    for attr_name, attr_value in attrs_list[i]:
        print(f"attr {attr_name} = {attr_value}")
    for prop_name, prop_value in props_list[i]:
        print(f"prop {prop_name} = {prop_value}")
```
