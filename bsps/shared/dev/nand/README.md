Version
=======
The information in this file describes the source of files in
bsps/shared/dev/nand/ and bsps/include/dev/nand/.

Import from:

  https://github.com/Xilinx/embeddedsw.git

```
commit 5330a64c8efd14f0eef09befdbb8d3d738c33ec2
Refs: <xilinx_v2022.2>
Author:     Nicole Baze <nicole.baze@xilinx.com>
AuthorDate: Mon Oct 3 13:27:19 2022 -0700
Commit:     Siva Addepalli <sivaprasad.addepalli@xilinx.com>
CommitDate: Fri Oct 7 10:26:16 2022 +0530

    xilpm: versal: server: Fix bug in AIE2 zeroization

    There is a bug in AIE2 zeriozation function when polling for memory
    zeroization complete. Currently the entire memory register is being
    checked against zero but instead we need to check the bits specific
    to the memory tiles. This patch updates the zeroization check by
    adding a mask so that only the desired bits are checked for zero.

    Signed-off-by: Nicole Baze <nicole.baze@xilinx.com>
    Acked-by: Jesus De Haro <jesus.de-haro@xilinx.com>
```
