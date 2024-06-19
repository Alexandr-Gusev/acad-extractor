from setuptools import Extension, setup


setup(
    ext_modules=[
        Extension(
            name="acad_extractor",
            py_limited_api=True,
            sources=["acad-extractor.cpp"],
            include_dirs = ["../../cpp"],
            libraries=["oleaut32", "ole32", "acad-extractor-late-binding-static"],
            library_dirs = ["../../cpp/build"],
        ),
    ],
)
