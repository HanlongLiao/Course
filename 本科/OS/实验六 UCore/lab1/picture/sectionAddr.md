磁盘IO地址和对应功能
<table>
<tr><td>IO地址</td><td>功能</td></tr>
<tr><td>0x1f0</td><td>读数据，当0x1f7不为忙状态时，可以读。</td></tr>
<tr><td>0x1f2</td><td>要读写的扇区数，每次读写前，你需要表明你要读写几个扇区。最小是1个扇区</td></tr>
<tr><td>0x1f3</td><td>如果是LBA模式，就是LBA参数的0-7位</td></tr>
<tr><td>0x1f4</td><td>如果是LBA模式，就是LBA参数的8-15位</td></tr>
<tr><td>0x1f5</td><td>如果是LBA模式，就是LBA参数的16-23位</td></tr>
<tr><td>0x1f6</td><td>第0~3位：如果是LBA模式就是24-27位     第4位：为0主盘；为1从盘</td></tr>
第6位：为1=LBA模式；0 = CHS模式        第7位和第5位必须为1</td></tr>
<tr><td>0x1f7</td><td>状态和命令寄存器。操作时先给命令，再读取，如果不是忙状态就从0x1f0端口读数据</td></tr>
</table>