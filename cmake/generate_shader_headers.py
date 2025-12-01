import os.path


class ShaderHeader:
    def __init__(self):
        self.vertex_lines = []
        self.fragment_lines = []
        self.compute_lines = []


def read_shader(filename: str, header: ShaderHeader) -> None:
    current_section = None
    with open(filename, "r", encoding="utf-8") as shader:
        line = shader.readline()
        while line:
            if line.find("#[vertex]") != -1:
                current_section = "vertex"
                line = shader.readline()
                continue
            if line.find("#[fragment]") != -1:
                current_section = "fragment"
                line = shader.readline()
                continue
            if line.find("#[compute]") != -1:
                current_section = "compute"
                line = shader.readline()
                continue
            line = line.replace("\r", "").replace("\n", "")
            if current_section == "vertex":
                header.vertex_lines += [line]
            if current_section == "fragment":
                header.fragment_lines += [line]
            if current_section == "compute":
                header.compute_lines += [line]
            line = shader.readline()


def c_str(lines: list[str]) -> str:
    while lines and lines[0] == "":
        lines.pop(0)
    while lines and lines[-1] == "":
        lines.pop(-1)

    result = ""
    for i, line in enumerate(lines):
        escaped_line = line.replace("\\", "\\\\").replace('"', '\\"')
        if i < len(lines) - 1:
            result += f'    "{escaped_line}\\n"\n'
        else:
            result += f'    "{escaped_line}\\n"'
    return result


def generate_header(filename: str, shader: str) -> None:
    read_shader(shader, header := ShaderHeader())
    name = os.path.basename(shader).replace(".glsl", "")
    with open(filename, "w", encoding="utf-8") as file:
        file.write(f"""\
/* GENERATED FILE -- DO NOT EDIT. */

#pragma once
 
""")
        if header.compute_lines:
            file.write(f"""\
static const char* {name}_vertex_source = 0;

static const char* {name}_fragment_source = 0;

static const char* {name}_compute_source = {{
{c_str(header.compute_lines)}
}};
""")
        else:
            file.write(f"""\
static const char* {name}_vertex_source = {{
{c_str(header.vertex_lines)}
}};

static const char* {name}_fragment_source = {{
{c_str(header.fragment_lines)}
}};

static const char* {name}_compute_source = 0;
""")


if __name__ == "__main__":
    import sys

    input_path = sys.argv[1]
    generate_header(f"{input_path}.gen.h", input_path)
