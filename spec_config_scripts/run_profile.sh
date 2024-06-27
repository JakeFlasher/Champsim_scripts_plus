# Perlbench
source shrc

go 500 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/perlbench_r_base.x86_64-m64 -- -I./lib splitmail.pl 6400 12 26 16 100 0   1>splitmail.6400.12.26.16.100.0.out 2>splitmail.6400.12.26.16.100.0.err &
pids[0]=$!
# GCC

go 502 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpugcc_r_base.x86_64-m64 -- ref32.c -O3 -fselective-scheduling -fselective-scheduling2 -o ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.s   1>ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.out 2>ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.err &
pids[1]=$!
# Bwaves

go 503 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/bwaves_r_base.x86_64-m64 -- bwaves_4  <bwaves_4.in 1>bwaves_4.out 2>bwaves_4.err &
pids[2]=$!
# MCF
go 505 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/mcf_r_base.x86_64-m64 -- inp.in    1>inp.out 2>inp.err & 
pids[3]=$!
# CactusBSSN
go 507 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cactusBSSN_r_base.x86_64-m64 -- spec_ref.par     1>spec_ref.out 2>spec_ref.err &
pids[4]=$!
# NAMD
go 508 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/namd_r_base.x86_64-m64 -- --input apoa1.input --output apoa1.ref.output --iterations 65   1>namd.out 2>namd.err &
pids[5]=$!
# PAREST
go 510 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/parest_r_base.x86_64-m64 -- ref.prm   1>ref.out 2>ref.err &
pids[6]=$!


# POV-Ray
go 511 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/povray_r_base.x86_64-m64 -- SPEC-benchmark-ref.ini   1>SPEC-benchmark-ref.stdout 2>SPEC-benchmark-ref.stderr &
pids[7]=$!
# LBM
go 519 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/lbm_r_base.x86_64-m64 -- 3000 reference.dat 0 0 100_100_130_ldc.of   1>lbm.out 2>lbm.err &
pids[8]=$!
# OMNeT++
go 520 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/omnetpp_r_base.x86_64-m64 -- -c General -r 0   1>omnetpp.General-0.out 2>omnetpp.General-0.err &
pids[9]=$!
# WRF
go 521 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/wrf_r_base.x86_64-m64 --   1>rsl.out.0000 2>wrf.err &
pids[10]=$!
# Xalan
go 523 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpuxalan_r_base.x86_64-m64 -- -v t5.xml xalanc.xsl   1>ref-t5.out 2>ref-t5.err &
pids[11]=$!
# x264

go 525 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/x264_r_base.x86_64-m64 -- --seek 500 --dumpyuv 200 --frames 1250 -o BuckBunny_New.264 BuckBunny.yuv 1280x720   1>run_0500-1250_x264_r_base.x86_64-m64 --_x264.out 2>run_0500-1250_x264_r_base.x86_64-m64 --_x264.err &
pids[12]=$!
# Blender
go 526 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/blender_r_base.x86_64-m64 -- sh3_no_char.blend --render-output sh3_no_char_ --threads 1 -b -F RAWTGA -s 849 -e 849 -a   1>sh3_no_char.849.spec.out 2>sh3_no_char.849.spec.err &
pids[13]=$!
# CAM4
go 527 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cam4_r_base.x86_64-m64 --   1>cam4_r_base.x86_64-m64 --.txt 2>cam4_r_base.x86_64-m64 --.err &
pids[14]=$!
# DeepSJeng
go 531 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/deepsjeng_r_base.x86_64-m64 -- ref.txt   1>ref.out 2>ref.err &
pids[15]=$!
# ImageMagick
go 538 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/imagick_r_base.x86_64-m64 -- -limit disk 0 refrate_input.tga -edge 41 -resample 181% -emboss 31 -colorspace YUV -mean-shift 19x19+15% -resize 30% refrate_output.tga   1>refrate_convert.out 2>refrate_convert.err &
pids[16]=$!
# Leela Chess Zero
go 541 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/leela_r_base.x86_64-m64 -- ref.sgf   1>ref.out 2>ref.err &
pids[17]=$!
# NAB
go 544 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/nab_r_base.x86_64-m64 -- 1am0 1122214447 122   1>1am0.out 2>1am0.err &
pids[18]=$!
# Exchange2
go 548 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/exchange2_r_base.x86_64-m64 -- 6   1>exchange2.txt 2>exchange2.err &
pids[19]=$!
# Fotonik3D
go 549 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/fotonik3d_r_base.x86_64-m64 -- --  1>fotonik3d_r.log 2>fotonik3d_r.err &
pids[20]=$!
# ROMS
go 554 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/roms_r_base.x86_64-m64 --  <ocean_benchmark2.in.x 1>ocean_benchmark2.log 2>ocean_benchmark2.err &
pids[21]=$!
# XZ

go 557 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/xz_r_base.x86_64-m64 -- input.combined.xz 250 a841f68f38572a49d86226b7ff5baeb31bd19dc637a922a972b2e6d1257a890f6a544ecab967c313e370478c74f760eb229d4eef8a8d2836d233d3e9dd1430bf 40401484 41217675 7   1>input.combined-250-7.out 2>input.combined-250-7.err &
pids[22]=$!


for pid in ${pids[*]}; do
    wait $pid
done
