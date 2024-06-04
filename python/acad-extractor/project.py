from sipbuild import Option, Project

class AeProject(Project):
    def get_options(self):
        options = super().get_options()

        ae_variant = Option(
            "ae_variant",
            help="static, shared, src",
            default="static"
        )
        options.append(ae_variant)

        ae_binding = Option(
            "ae_binding",
            help="early, late",
            default="late"
        )
        options.append(ae_binding)

        return options

    def update(self, tool):
        bindings = self.bindings["acad-extractor"]

        if self.ae_variant == "static":
            bindings.libraries.append(f"acad-extractor-{self.ae_binding}-binding-static")
        elif self.ae_variant == "shared":
            self.wheel_includes = [f"../../cpp/build/acad-extractor-{self.ae_binding}-binding-shared.dll"]
            bindings.libraries.append(f"acad-extractor-{self.ae_binding}-binding-shared")
            bindings.extra_compile_args = ["-DAE_IMPORT"]
        elif self.ae_variant == "src":
            bindings.sources = ["../../cpp/core.cpp", "../../cpp/utils.cpp"]
            if self.ae_binding == "early":
                bindings.extra_compile_args = ["-DAE_EARLY_BINDING"]

        super().update(tool)
