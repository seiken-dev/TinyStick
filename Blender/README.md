## Blenderのデータについて
Blenderのデータは、各パーツを結合する前のものです。Blender(3.6)の結合には次の２通りの方法があります。
- **Ctrl-J**<br>
重なった面は重複したまま残ります。
- **Bool Tool**の**Union**<br>
重なった面は削除されますが、複雑な形状だと不具合が起こりがち。

JLCPCBの[3Dプリントサービス](https://jlc3dp.com/)では、重複した面があると警告が出ますが、そのまま印刷しても問題ないようです。**Case**,**CaseBtm**のSTL出力前に、その他のパーツを上記いづれかの方法で結合してください。

### Case
- BtnEmboss1-4
- SW5Button
- PCB.Knob, 001
- PCB.Pole, 001

### CaseBtm
- CaseSide
- Edge, 001, 002<br>
  (この２つをunion)
- CaseBtmB.Knob, 001
- CaseBtmF.Knob, 001
- CaseBtmF.Pad
- MagHolder, 001, 002
