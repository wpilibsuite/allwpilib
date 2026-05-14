ExpansionHubVelocityConstants(int hubNumber, int motorNumber) {
  NetworkTableInstance systemServer = SystemServer.getSystemServer();

  PubSubOption[] options =
      new PubSubOption[] {
        PubSubOption.SEND_ALL,
        PubSubOption.KEEP_DUPLICATES,
        PubSubOption.periodic(0.005)
      };

  m_pPublisher =
      systemServer
          .getDoubleTopic(
              "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/velocity/kp")
          .publish(options);

  m_iPublisher =
      systemServer
          .getDoubleTopic(
              "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/velocity/ki")
          .publish(options);

  m_dPublisher =
      systemServer
          .getDoubleTopic(
              "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/velocity/kd")
          .publish(options);

  m_pPublisher.setDefault(1.0);
  m_iPublisher.setDefault(0.0);
  m_dPublisher.setDefault(0.01);

  m_sPublisher =
      systemServer
          .getDoubleTopic(
              "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/velocity/ks")
          .publish(options);

  m_vPublisher =
      systemServer
          .getDoubleTopic(
              "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/velocity/kv")
          .publish(options);

  m_aPublisher =
      systemServer
          .getDoubleTopic(
              "/rhsp/" + hubNumber + "/motor" + motorNumber + "/constants/velocity/ka")
          .publish(options);
}
