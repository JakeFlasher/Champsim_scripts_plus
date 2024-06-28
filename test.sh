# Perlbench
source shrc

go 500 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/perlbench_r_base.x86_64-m64 -- -I./lib splitmail.pl 6400 12 26 16 100 0   &
pids[0]=$!
# GCC

go 502 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpugcc_r_base.x86_64-m64 -- ref32.c -O3 -fselective-scheduling -fselective-scheduling2 -o ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.s   &
pids[1]=$!
# Bwaves

go 503 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/bwaves_r_base.x86_64-m64 -- bwaves_4  <bwaves_4.in &
pids[2]=$!
# MCF
go 505 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/mcf_r_base.x86_64-m64 -- inp.in    & 
pids[3]=$!
# CactusBSSN
go 507 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cactusBSSN_r_base.x86_64-m64 -- spec_ref.par     &
pids[4]=$!
# NAMD
go 508 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/namd_r_base.x86_64-m64 -- --input apoa1.input --output apoa1.ref.output --iterations 65   &
pids[5]=$!
# PAREST
go 510 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/parest_r_base.x86_64-m64 -- ref.prm   &
pids[6]=$!


# POV-Ray
go 511 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/povray_r_base.x86_64-m64 -- SPEC-benchmark-ref.ini   &
pids[7]=$!
# LBM
go 519 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/lbm_r_base.x86_64-m64 -- 3000 reference.dat 0 0 100_100_130_ldc.of   &
pids[8]=$!
# OMNeT++
go 520 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/omnetpp_r_base.x86_64-m64 -- -c General -r 0   &
pids[9]=$!
# WRF
go 521 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/wrf_r_base.x86_64-m64 --   &
pids[10]=$!
# Xalan
go 523 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpuxalan_r_base.x86_64-m64 -- -v t5.xml xalanc.xsl   &
pids[11]=$!
# x264

go 525 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/x264_r_base.x86_64-m64 -- --seek 500 --dumpyuv 200 --frames 1250 -o BuckBunny_New.264 BuckBunny.yuv 1280x720   &
pids[12]=$!
# Blender
go 526 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/blender_r_base.x86_64-m64 -- sh3_no_char.blend --render-output sh3_no_char_ --threads 1 -b -F RAWTGA -s 849 -e 849 -a   &
pids[13]=$!
# CAM4
go 527 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cam4_r_base.x86_64-m64 --   &
pids[14]=$!
# DeepSJeng
go 531 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/deepsjeng_r_base.x86_64-m64 -- ref.txt   &
pids[15]=$!
# ImageMagick
go 538 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/imagick_r_base.x86_64-m64 -- -limit disk 0 refrate_input.tga -edge 41 -resample 181% -emboss 31 -colorspace YUV -mean-shift 19x19+15% -resize 30% refrate_output.tga   &
pids[16]=$!
# Leela Chess Zero
go 541 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/leela_r_base.x86_64-m64 -- ref.sgf   &
pids[17]=$!
# NAB
go 544 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/nab_r_base.x86_64-m64 -- 1am0 1122214447 122   &
pids[18]=$!
# Exchange2
go 548 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/exchange2_r_base.x86_64-m64 -- 6   &
pids[19]=$!
# Fotonik3D
go 549 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/fotonik3d_r_base.x86_64-m64 -- --  &
pids[20]=$!
# ROMS
go 554 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/roms_r_base.x86_64-m64 --  <ocean_benchmark2.in.x &
pids[21]=$!
# XZ

go 557 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/xz_r_base.x86_64-m64 -- input.combined.xz 250 a841f68f38572a49d86226b7ff5baeb31bd19dc637a922a972b2e6d1257a890f6a544ecab967c313e370478c74f760eb229d4eef8a8d2836d233d3e9dd1430bf 40401484 41217675 7   &
pids[22]=$!


for pid in ${pids[*]}; do
    wait $pid
done
