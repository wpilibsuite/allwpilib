
import os
import sys


def load_template(template_file):

    mapping = []

    with open(template_file, 'r') as f:
        for line in f.readlines():
            mapping.append(line.strip())

    return mapping


def generate_cpp_file(output_cpp_file, cpp_template, resource_types, instances):

    with open(output_cpp_file, 'w') as of, open(cpp_template, 'r') as tf:

         template = tf.read()
         java_resources = "\n".join(f"    {x}," for x in resource_types)
         java_instances = "\n".join(f"    {x}," for x in instances)
         gen_contents = template.replace("${usage_reporting_types_cpp}", java_resources)
         gen_contents = gen_contents.replace("${usage_reporting_instances_cpp}", java_instances)
         of.write(gen_contents)
         
def generate_java_file(output_java_file, java_template, resource_types, instances):

    with open(output_java_file, 'w') as of, open(java_template, 'r') as tf:

         template = tf.read()
         java_resources = "\n".join(f"    public static final int {x};" for x in resource_types)
         java_instances = "\n".join(f"    public static final int {x};" for x in instances)
         gen_contents = template.replace("${usage_reporting_types}", java_resources)
         gen_contents = gen_contents.replace("${usage_reporting_instances}", java_instances)
         of.write(gen_contents)




def main():
    dirname, _ = os.path.split(os.path.abspath(__file__))

    resource_template = os.path.join(dirname, "ResourceType.txt")
    instances_template = os.path.join(dirname, "Instances.txt")
    java_template = os.path.join(dirname, "FRCNetComm.java.in")
    cpp_template = os.path.join(dirname, "FRCUsageReporting.h.in")

    resources = load_template(resource_template)
    instances = load_template(instances_template)

    output_file = sys.argv[1]

    if "FRCUsageReporting.h" in output_file:
        generate_cpp_file(output_file, cpp_template, resources, instances)
    elif "." in output_file:
        generate_java_file(output_file, java_template, resources, instances)
    else:
        raise 


if __name__ == "__main__":
    main()
