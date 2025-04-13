import os
import subprocess
import re


def generate_and_embed(system_name: str, machine_name: str):
    builtin_dir = os.path.dirname(os.path.realpath(__file__))
    file_name = "fallback_device_api_wrappers"
    src_path = os.path.join(builtin_dir, f"{file_name}.cpp")
    dst_path = os.path.join(builtin_dir, f"{file_name}.{system_name}.{machine_name}.ll")
    subprocess.run(["clang++", "-c", "-emit-llvm", "-std=c++20", "-ffast-math", "-O3",
                    "-S", src_path, "-o", dst_path, "-fomit-frame-pointer",
                    "-fno-stack-protector", "-fno-rtti", "-fno-exceptions",
                    f"--target={machine_name}-unknown-{system_name}",
                    "-nostdinc", "-nostdlib", "-nostdinc++", "-nostdlib++"])
    with open(dst_path, "r", newline="\n") as f:
        content = "".join(line for line in f.readlines()
                          if not line.strip().startswith("@llvm.used") and
                          not line.strip().startswith("@llvm.compiler.used") and
                          not line.strip().startswith("; ModuleID") and
                          not line.strip().startswith("source_filename"))
        content = content.replace("\n\n\n", "\n\n")
    content = content.replace("define hidden", "define private")
    # find all @luisa_fallback_wrapper_(\w+) and collect in a list
    impl_prefix = "@luisa_fallback_"
    wrapper_prefix = "@luisa_fallback_wrapper_"
    functions = re.findall(f"{wrapper_prefix}([a-zA-Z0-9_]+)", content)
    for function_name in functions:
        dotted_name = function_name.replace("_", ".")
        print(f"{wrapper_prefix}{function_name} -> @luisa.{dotted_name}")
        content = content.replace(f"{wrapper_prefix}{function_name}(", f"@luisa.{dotted_name}(")
        content = content.replace(f"{impl_prefix}{function_name}(", f"@luisa.{dotted_name}.impl(")
    with open(dst_path, "w") as f:
        f.write(content)
    with open(os.path.join(builtin_dir, "..", "fallback_device_api_map_symbols.inl.h"), "w") as f:
        for function_name in functions:
            dotted_name = function_name.replace("_", ".")
            if f"luisa.{dotted_name}.impl(" in content:
                f.write(f"map_symbol(\"luisa.{dotted_name}.impl\", &api::luisa_fallback_{function_name});\n")
    # compile to bitcode
    try:
        bc_path = dst_path.replace(".ll", ".bc")
        subprocess.run(["llvm-as", dst_path, "-o", bc_path])
        with open(bc_path, "rb") as f:
            content = f.read()
    except:
        print("Failed to compile to bitcode.")
        content = content.encode("utf-8")
    with open(os.path.join(builtin_dir, f"{file_name}.{system_name}.{machine_name}.inl"), "wb") as f:
        data = [f"0x{c:02x}" for c in content]
        size = len(data)
        f.write(
            f'\nstatic const unsigned char luisa_fallback_backend_device_builtin_module[{size}] = {{\n'.encode("utf-8"))
        wrapped = ["    " + ", ".join(data[i: i + 16]) for i in range(0, len(data), 16)]
        f.write(",\n".join(wrapped).encode("utf-8"))
        f.write("\n};\n".encode("utf-8"))


if __name__ == "__main__":
    for system in ["linux", "windows", "darwin"]:
        for machine in ["x86_64", "arm64"]:
            generate_and_embed(system, machine)

