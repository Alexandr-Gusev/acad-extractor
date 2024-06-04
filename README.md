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
dt = select_on_screen(names, attrs_list, props_list)
print(f"{len(names)} at {dt} ms")
for i, name in enumerate(names):
    print("-" * 70)
    print(name)
    for name, value in attrs_list[i]:
        print(f"attr {name} = {value}")
    for name, value in props_list[i]:
        print(f"prop {name} = {value}")
```
