import re
import os


URL_REGEX = re.compile("((https?):((//)|(\\\\))+([\w\d:#@%/;$()~_?\+-=\\\.&](#!)?)*)", re.DOTALL)

# Grab RemoteLiteralIncludes from .rst resources
# Return the URLs in the RLIs
def grab_rli_urls(path: str):
    # retrieve list of rst documents
    documents = {}
    for root, subdirs, files in os.walk(os.path.abspath(path)):
        for document in files:
            if ".rst" in document:
                documents[os.path.join(root, document)] = os.path.join(root, document)

    # retrieve list of rlis
    rli_urls = {}
    for document in documents:
        with open(documents[document], "r", encoding="utf8") as parsed_document:
            for num, line in enumerate(parsed_document):
                if ".. remoteliteralinclude" in line or ".. rli" in line:
                    url = re.findall(URL_REGEX, line)[0][0]

                    # only run check on allwpilib lines
                    if "allwpilib" in line:
                        rli_urls[document + "#" + str(num)] = url

    return rli_urls


def main():
    urls = grab_rli_urls("frc-docs")
    lines = []
    for root, subdirs, files in os.walk(os.path.abspath("./")):
        if "frc-docs" in root:
            continue
        for line in urls:
            for name in files:
                if name in urls[line]:
                    lines.append(line.split("allwpilib" + os.path.sep)[2])

    modified = False
    with open("output.md", "w") as f:
        f.write("# Documentation Updates Needed!\n")
        f.write("The following links in the [frc-docs](https://github.com/wpilibsuite/frc-docs) repository need to be updated\n")
        f.write("```\n")

        for line in lines:
            f.write(line + "\n")
        
        f.write("```\n")
        f.write("frc-docs literalinclude report")
        
        print("Documentation updates are needed! \n")
        modified = True

    if not modified:
        print("No changes are necessary")


if __name__ == '__main__':
    main()
