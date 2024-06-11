#!/bin/bash
# sizeof returned: 64 for trace inst type
# raw
../qemu/build/qemu-x86_64 -D log.txt -d plugin -plugin build/libbbv_ibar.so,name=/mnt/d/Stockfish_bbv,size=100000000 -- /usr/games/stockfish bench 128 1 24 default depth

../Simpoint.3.2_fix/bin/simpoint -maxK 10 -loadFVFile /mnt/d/Stockfish_bbv/bbv -saveSimpoints /mnt/d/Stockfish_bbv/simpoints -saveSimpointWeights /mnt/d/Stockfish_bbv/weights

env TRACE_FILENAME=/mnt/d/Stockfish_bbv/champsim.trace SIMPOINT_FILE= /mnt/d/Stockfish_bbv/simpoints BB_INTERVAL=100000000  ../qemu/build/qemu-x86_64 -D log.txt -d plugin -plugin build/libchampsim.so -- /usr/games/stockfish bench 128 1 24 default depth
set -e

output=$1
# cmd="~/coremark/coremark-aarch64-linux-gnu.exe 0 0 0 10000 > /dev/null"
cmd="${@:2}"
bb_interval=100000000

CMD_GETBBV="~/qemu/build/qemu-loongarch64 -D log.txt -d plugin -plugin ~/qemu_plugins/build/libbbv_ibar.so,name=${output},size=${bb_interval} -- $cmd"

echo ${CMD_GETBBV}
eval ${CMD_GETBBV}

CMD_GETSIMPOINT="/ht/320/btracer/SimPoint.3.2/bin/simpoint -maxK 5 -loadFVFile ${output}/bbv -saveSimpoints ${output}/simpoints -saveSimpointWeights ${output}/weights >/dev/null"
echo ${CMD_GETSIMPOINT}
eval ${CMD_GETSIMPOINT}

CMD_GETTRACE="env TRACE_FILENAME=${output}/champsim.trace SIMPOINT_FILE=${output}/simpoints BB_INTERVAL=${bb_interval} ~/qemu/build/qemu-loongarch64 -D log.txt -d plugin -plugin ~/qemu_plugins/build/libchampsim_la_with_reg_simpoint.so -- ${cmd}"
echo ${CMD_GETTRACE}
eval ${CMD_GETTRACE}
