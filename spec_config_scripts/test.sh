# Perlbench
source shrc
go 502 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpugcc_r_base.x86_64-m64 -- gcc-pp.c -O2 -finline-limit=36000 -fpic -o gcc-pp.opts-O2_-finline-limit_36000_-fpic.s   1>gcc-pp.opts-O2_-finline-limit_36000_-fpic.out 2>gcc-pp.opts-O2_-finline-limit_36000_-fpic.err
go 502 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpugcc_r_base.x86_64-m64 -- gcc-smaller.c -O3 -fipa-pta -o gcc-smaller.opts-O3_-fipa-pta.s   1>gcc-smaller.opts-O3_-fipa-pta.out 2>gcc-smaller.opts-O3_-fipa-pta.err
go 502 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpugcc_r_base.x86_64-m64 -- ref32.c -O5 -o ref32.opts-O5.s   1>ref32.opts-O5.out 2>ref32.opts-O5.err
go 502 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpugcc_r_base.x86_64-m64 -- ref32.c -O3 -fselective-scheduling -fselective-scheduling2 -o ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.s   1>ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.out 2>ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.err

# Bwaves
go 503 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/bwaves_r_base.x86_64-m64 -- bwaves_1  <bwaves_1.in 1>bwaves_1.out 2>bwaves_1.err
go 503 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/bwaves_r_base.x86_64-m64 -- bwaves_2  <bwaves_2.in 1>bwaves_2.out 2>bwaves_2.err
go 503 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/bwaves_r_base.x86_64-m64 -- bwaves_3  <bwaves_3.in 1>bwaves_3.out 2>bwaves_3.err
go 503 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/bwaves_r_base.x86_64-m64 -- bwaves_4  <bwaves_4.in 1>bwaves_4.out 2>bwaves_4.err

# MCF
go 505 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/mcf_r_base.x86_64-m64 -- inp.in    1>inp.out 2>inp.err

# CactusBSSN
go 507 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cactusBSSN_r_base.x86_64-m64 -- spec_ref.par     1>spec_ref.out 2>spec_ref.err

# NAMD
go 508 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/namd_r_base.x86_64-m64 -- --input apoa1.input --output apoa1.ref.output --iterations 65   1>namd.out 2>namd.err

# PAREST
go 510 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/parest_r_base.x86_64-m64 -- ref.prm   1>ref.out 2>ref.err

# POV-Ray
go 511 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/povray_r_base.x86_64-m64 -- SPEC-benchmark-ref.ini   1>SPEC-benchmark-ref.stdout 2>SPEC-benchmark-ref.stderr

# LBM
go 519 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/lbm_r_base.x86_64-m64 -- 3000 reference.dat 0 0 100_100_130_ldc.of   1>lbm.out 2>lbm.err

# OMNeT++
go 520 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/omnetpp_r_base.x86_64-m64 -- -c General -r 0   1>omnetpp.General-0.out 2>omnetpp.General-0.err

# WRF
go 521 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/wrf_r_base.x86_64-m64 --   1>rsl.out.0000 2>wrf.err

# Xalan
go 523 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpuxalan_r_base.x86_64-m64 -- -v t5.xml xalanc.xsl   1>ref-t5.out 2>ref-t5.err

# x264
go 525 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/x264_r_base.x86_64-m64 -- --pass 1 --stats x264_stats.log --bitrate 1000 --frames 1000 -o BuckBunny_New.264 BuckBunny.yuv 1280x720   1>run_000-1000_x264_r_base.x86_64-m64 --_x264_pass1.out 2>run_000-1000_x264_r_base.x86_64-m64 --_x264_pass1.err
go 525 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/x264_r_base.x86_64-m64 -- --pass 2 --stats x264_stats.log --bitrate 1000 --dumpyuv 200 --frames 1000 -o BuckBunny_New.264 BuckBunny.yuv 1280x720   1>run_000-1000_x264_r_base.x86_64-m64 --_x264_pass2.out 2>run_000-1000_x264_r_base.x86_64-m64 --_x264_pass2.err

# x264 (continued)
go 525 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/x264_r_base.x86_64-m64 -- --seek 500 --dumpyuv 200 --frames 1250 -o BuckBunny_New.264 BuckBunny.yuv 1280x720   1>run_0500-1250_x264_r_base.x86_64-m64 --_x264.out 2>run_0500-1250_x264_r_base.x86_64-m64 --_x264.err

# Blender
go 526 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/blender_r_base.x86_64-m64 -- sh3_no_char.blend --render-output sh3_no_char_ --threads 1 -b -F RAWTGA -s 849 -e 849 -a   1>sh3_no_char.849.spec.out 2>sh3_no_char.849.spec.err

# CAM4
go 527 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cam4_r_base.x86_64-m64 --   1>cam4_r_base.x86_64-m64 --.txt 2>cam4_r_base.x86_64-m64 --.err

# DeepSJeng
go 531 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/deepsjeng_r_base.x86_64-m64 -- ref.txt   1>ref.out 2>ref.err

# ImageMagick
go 538 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/imagick_r_base.x86_64-m64 -- -limit disk 0 refrate_input.tga -edge 41 -resample 181% -emboss 31 -colorspace YUV -mean-shift 19x19+15% -resize 30% refrate_output.tga   1>refrate_convert.out 2>refrate_convert.err

# Leela Chess Zero
go 541 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/leela_r_base.x86_64-m64 -- ref.sgf   1>ref.out 2>ref.err

# NAB
go 544 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/nab_r_base.x86_64-m64 -- 1am0 1122214447 122   1>1am0.out 2>1am0.err

# Exchange2
go 548 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/exchange2_r_base.x86_64-m64 -- 6   1>exchange2.txt 2>exchange2.err

# Fotonik3D
go 549 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/fotonik3d_r_base.x86_64-m64 -- --  1>fotonik3d_r.log 2>fotonik3d_r.err

# ROMS
go 554 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/roms_r_base.x86_64-m64 --  <ocean_benchmark2.in.x 1>ocean_benchmark2.log 2>ocean_benchmark2.err

# XZ
go 557 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/xz_r_base.x86_64-m64 -- cld.tar.xz 160 19cf30ae51eddcbefda78dd06014b4b96281456e078ca7c13e1c0c9e6aaea8dff3efb4ad6b0456697718cede6bd5454852652806a657bb56e07d61128434b474 59796407 61004416 6   1>cld.tar-160-6.out 2>cld.tar-160-6.err
go 557 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/xz_r_base.x86_64-m64 -- cpu2006docs.tar.xz 250 055ce243071129412e9dd0b3b69a21654033a9b723d874b2015c774fac1553d9713be561ca86f74e4f16f22e664fc17a79f30caa5ad2c04fbc447549c2810fae 23047774 23513385 6e   1>cpu2006docs.tar-250-6e.out 2>cpu2006docs.tar-250-6e.err
go 557 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/xz_r_base.x86_64-m64 -- input.combined.xz 250 a841f68f38572a49d86226b7ff5baeb31bd19dc637a922a972b2e6d1257a890f6a544ecab967c313e370478c74f760eb229d4eef8a8d2836d233d3e9dd1430bf 40401484 41217675 7   1>input.combined-250-7.out 2>input.combined-250-7.err

# SPECrand_fr
go 997 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/specrand_fr_base.x86_64-m64 --  1>specrand_fr.out 2>specrand_fr.err

# SPECrand_ir
go 999 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/trace/target/release/trace plugin -d -t "/root/developing/trace/plugins/jade_trace_export/target/release/libjade_trace_export.so -o \./trace" -l 1000000000 ../run_base_refrate_x86_64-m64.0000/specrand_ir_base.x86_64-m64 --  1>specrand_ir.out 2>specrand_ir.err
