import os
import sys


def main():
    MAX_NUM = 20

    dirname, _ = os.path.split(os.path.abspath(__file__))
    cmake_binary_dir = sys.argv[1]

    with open(f"{dirname}/src/generate/GenericNumber.java.in",
              "r") as templateFile:
        template = templateFile.read()
        rootPath = f"{cmake_binary_dir}/generated/main/java/edu/wpi/first/wpiutil/math/numbers"

        if not os.path.exists(rootPath):
            os.makedirs(rootPath)

        for i in range(MAX_NUM + 1):
            new_contents = template.replace("${num}", str(i))

            if os.path.exists(f"{rootPath}/N{i}.java"):
                with open(f"{rootPath}/N{i}.java", "r") as f:
                    if f.read() == new_contents:
                        continue

            # File either doesn't exist or has different contents
            with open(f"{rootPath}/N{i}.java", "w") as f:
                f.write(new_contents)

    with open(f"{dirname}/src/generate/Nat.java.in", "r") as templateFile:
        template = templateFile.read()
        outputPath = f"{cmake_binary_dir}/generated/main/java/edu/wpi/first/wpiutil/math/Nat.java"
        with open(f"{dirname}/src/generate/NatGetter.java.in",
                  "r") as getterFile:
            getter = getterFile.read()

        importsString = ""

        for i in range(MAX_NUM + 1):
            importsString += f"import edu.wpi.first.wpiutil.math.numbers.N{i};\n"
            template += getter.replace("${num}", str(i))

        template += "}\n"

        template = template.replace('{{REPLACEWITHIMPORTS}}', importsString)

        if os.path.exists(outputPath):
            with open(outputPath, "r") as f:
                if f.read() == template:
                    return 0

        # File either doesn't exist or has different contents
        with open(outputPath, "w") as f:
            f.write(template)


if __name__ == "__main__":
    main()
