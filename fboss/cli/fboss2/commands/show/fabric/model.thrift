namespace cpp2 facebook.fboss.cli

struct ShowFabricModel {
  1: list<FabricEntry> fabricEntries;
}

struct FabricEntry {
  1: string localPort;
  2: string remoteSwitchName;
  3: i64 remoteSwitchId;
  4: string remotePortName;
  5: i32 remotePortId;
}
