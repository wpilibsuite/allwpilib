# datalog

DataLog is a simple, fast, binary logging format designed to be more advanced than logging converting values to strings, while maintaining speed on embedded platforms. More about the motivation can be found in [the DataLog format specification](./doc/datalog.adoc).

Also see [the Kaitai Struct file](./doc/wpilog.ksy), which uses [Kaitai Struct](https://kaitai.io/) to declaratively describe the DataLog format and allows for generating libraries in other languages to parse DataLog files.
