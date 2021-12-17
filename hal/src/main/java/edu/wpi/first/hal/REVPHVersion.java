package edu.wpi.first.hal;

@SuppressWarnings("AbbreviationAsWordInName")
public class REVPHVersion {
  @SuppressWarnings("MemberName")
  public int firmwareMajor;

  @SuppressWarnings("MemberName")
  public int firmwareMinor;

  @SuppressWarnings("MemberName")
  public int firmwareFix;

  @SuppressWarnings("MemberName")
  public int hardwareMinor;

  @SuppressWarnings("MemberName")
  public int hardwareMajor;

  @SuppressWarnings("MemberName")
  public int uniqueId;

  /**
   * Constructs a revph version (Called from the HAL).
   *
   * @param firmwareMajor firmware major
   * @param firmwareMinor firmware minor
   * @param firmwareFix firmware fix
   * @param hardwareMinor hardware minor
   * @param hardwareMajor hardware major
   * @param uniqueId unique id
   */
  public REVPHVersion(
      int firmwareMajor,
      int firmwareMinor,
      int firmwareFix,
      int hardwareMinor,
      int hardwareMajor,
      int uniqueId) {
    this.firmwareMajor = firmwareMajor;
    this.firmwareMinor = firmwareMinor;
    this.firmwareFix = firmwareFix;
    this.hardwareMinor = hardwareMinor;
    this.hardwareMajor = hardwareMajor;
    this.uniqueId = uniqueId;
  }
}
