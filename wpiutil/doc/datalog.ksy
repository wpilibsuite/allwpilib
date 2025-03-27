meta:
  id: wpilog
  title: WPILib Data Log
  file-extension: wpilog
  ks-version: "0.10"
  endian: le
  bit-endian: le
seq:
  - id: header
    type: header
  - id: records
    type: record
    repeat: eos
types:
  version_number:
    seq:
      - id: minor
        type: u1
      - id: major
        type: u1
  header:
    seq:
      - id: magic
        contents: WPILOG
      - id: version
        type: version_number
      - id: extra_header_length
        type: u4
      - id: extra_header
        size: extra_header_length
        type: str
        encoding: UTF-8
  record:
    seq:
      - id: header_length
        type: record_header_length
      - id: entry_id
        type:
          switch-on: header_length.len_entry_id
          cases:
            0: u1
            1: u2
            2: b24
            3: u4
      - id: len_payload
        type:
          switch-on: header_length.len_payload_size
          cases:
            0: u1
            1: u2
            2: b24
            3: u4
      - id: timestamp
        type:
          switch-on: header_length.len_timestamp
          cases:
            0: u1
            1: u2
            2: b24
            3: u4
            4: b40
            5: b48
            6: b56
            7: u8
      - id: payload
        size: len_payload
        type:
          switch-on: entry_id
          cases:
            # ID 0 is reserved for control records
            0: control_record_payload
  record_header_length:
    seq:
      - id: len_entry_id
        type: b2
      - id: len_payload_size
        type: b2
      - id: len_timestamp
        type: b3
      - id: spare_bit
        type: b1
  control_record_payload:
    seq:
      - id: type
        type: u1
        enum: control_record_type
      - id: data
        type:
          switch-on: type
          cases:
            "control_record_type::start": control_record_start_data
            "control_record_type::finish": control_record_finish_data
            "control_record_type::set_metadata": control_record_set_metadata_data
  control_record_start_data:
    seq:
      - id: entry_id
        type: u4
      - id: len_entry_name
        type: u4
      - id: entry_name
        size: len_entry_name
        type: str
        encoding: UTF-8
      - id: len_entry_type
        type: u4
      - id: entry_type
        size: len_entry_type
        type: str
        encoding: UTF-8
      - id: len_entry_metadata
        type: u4
      - id: entry_metadata
        size: len_entry_metadata
        type: str
        encoding: UTF-8
  control_record_finish_data:
    seq:
      - id: entry_id
        type: u4
  control_record_set_metadata_data:
    seq:
      - id: entry_id
        type: u4
      - id: len_entry_metadata
        type: u4
      - id: entry_metadata
        size: len_entry_metadata
        type: str
        encoding: UTF-8
enums:
  control_record_type:
    0: start
    1: finish
    2: set_metadata
