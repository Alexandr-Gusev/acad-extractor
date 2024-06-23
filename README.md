# acad-extractor

A quick tool to extract block attributes and properties from AutoCAD drawings.

## Installation

```bash
pip install acad-extractor
```

## Usage

```python
from acad_extractor import select_on_screen

print("Select AutoCAD blocks on screen")
names = []
attrs_list = []
props_list = []
handles = []
points = []
def callback(processed, total):
    print(f"Processed {processed}/{total}")
print("-" * 70)
dt = select_on_screen(names, attrs_list, props_list, handles, callback, points)
print("-" * 70)
print(f"Processed {len(names)} at {dt} ms")
for i, name in enumerate(names):
    print("-" * 70)
    print(f"{name} {handles[i]} ({points[i][0]}, {points[i][1]}, {points[i][2]})")
    for attr_name, attr_value in attrs_list[i]:
        print(f"attr {attr_name} = {attr_value}")
    for prop_name, prop_value in props_list[i]:
        print(f"prop {prop_name} = {prop_value}")
```
