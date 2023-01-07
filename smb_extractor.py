with open("smb.nes", "rb") as i:
    with open("smb.chr", "wb") as o:
        raw = i.read();
        data = raw[0x008010:];
        print("CHR size for loading " + str(len(data)))
        o.write(raw[0x008010:]);
