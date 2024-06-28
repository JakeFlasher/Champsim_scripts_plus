# Perlbench
source shrc

go 500 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 907B_trace" -f 907000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/perlbench_r_base.x86_64-m64 -- -I./lib splitmail.pl 6400 12 26 16 100 0   &
go 500 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 1657B_trace" -f 1657000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/perlbench_r_base.x86_64-m64 -- -I./lib splitmail.pl 6400 12 26 16 100 0   &
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 285B_trace" -f 285000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/perlbench_r_base.x86_64-m64 -- -I./lib splitmail.pl 6400 12 26 16 100 0   &
 
# GCC

go 502 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 524B_trace" -f 524000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpugcc_r_base.x86_64-m64 -- ref32.c -O3 -fselective-scheduling -fselective-scheduling2 -o ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.s   &
go 502 
cd run/run_base_refrate_x86_64-m64.0000 && 
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 703B_trace" -f 703000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpugcc_r_base.x86_64-m64 -- ref32.c -O3 -fselective-scheduling -fselective-scheduling2 -o ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.s   &
go 502 
cd run/run_base_refrate_x86_64-m64.0000 && 
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 24B_trace" -f 24000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpugcc_r_base.x86_64-m64 -- ref32.c -O3 -fselective-scheduling -fselective-scheduling2 -o ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.s   &
go 502 
cd run/run_base_refrate_x86_64-m64.0000 && 
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 241B_trace" -f 241000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpugcc_r_base.x86_64-m64 -- ref32.c -O3 -fselective-scheduling -fselective-scheduling2 -o ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.s   &
pids[1]=$!
# Bwaves

go 503 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 310B_trace" -f 310000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/bwaves_r_base.x86_64-m64 -- bwaves_4  <bwaves_4.in &
go 503 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 491B_trace" -f 491000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/bwaves_r_base.x86_64-m64 -- bwaves_4  <bwaves_4.in &
go 503 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 727B_trace" -f 727000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/bwaves_r_base.x86_64-m64 -- bwaves_4  <bwaves_4.in &
go 503 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 267B_trace" -f 267000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/bwaves_r_base.x86_64-m64 -- bwaves_4  <bwaves_4.in &
pids[2]=$!
# MCF
go 505 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 612B_trace" -f 612000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/mcf_r_base.x86_64-m64 -- inp.in    & 
go 505 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 775B_trace" -f 775000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/mcf_r_base.x86_64-m64 -- inp.in    & 
go 505 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 911B_trace" -f 911000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/mcf_r_base.x86_64-m64 -- inp.in    & 
go 505 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 505B_trace" -f 505000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/mcf_r_base.x86_64-m64 -- inp.in    & 
go 505 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/omnitr_export/target/release/libjade_trace_export.so -o 130B_trace" -f 130000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/mcf_r_base.x86_64-m64 -- inp.in    & 
