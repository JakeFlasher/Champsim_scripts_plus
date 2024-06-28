# Perlbench
source shrc

go 500 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 907B_trace/rd_analysis_output.gz" -f 907000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/perlbench_r_base.x86_64-m64 -- -I./lib splitmail.pl 6400 12 26 16 100 0   &
go 500 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 1657B_trace/rd_analysis_output.gz" -f 1657000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/perlbench_r_base.x86_64-m64 -- -I./lib splitmail.pl 6400 12 26 16 100 0   &
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 285B_trace/rd_analysis_output.gz" -f 285000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/perlbench_r_base.x86_64-m64 -- -I./lib splitmail.pl 6400 12 26 16 100 0   &
 
# GCC

go 502 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 524B_trace/rd_analysis_output.gz" -f 524000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpugcc_r_base.x86_64-m64 -- ref32.c -O3 -fselective-scheduling -fselective-scheduling2 -o ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.s   &
go 502 
cd run/run_base_refrate_x86_64-m64.0000 && 
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 703B_trace/rd_analysis_output.gz" -f 703000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpugcc_r_base.x86_64-m64 -- ref32.c -O3 -fselective-scheduling -fselective-scheduling2 -o ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.s   &
go 502 
cd run/run_base_refrate_x86_64-m64.0000 && 
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 24B_trace/rd_analysis_output.gz" -f 24000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpugcc_r_base.x86_64-m64 -- ref32.c -O3 -fselective-scheduling -fselective-scheduling2 -o ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.s   &
go 502 
cd run/run_base_refrate_x86_64-m64.0000 && 
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 241B_trace/rd_analysis_output.gz" -f 241000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/cpugcc_r_base.x86_64-m64 -- ref32.c -O3 -fselective-scheduling -fselective-scheduling2 -o ref32.opts-O3_-fselective-scheduling_-fselective-scheduling2.s   &
pids[1]=$!
# Bwaves

go 503 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 310B_trace/rd_analysis_output.gz" -f 310000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/bwaves_r_base.x86_64-m64 -- bwaves_4  <bwaves_4.in &
go 503 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 491B_trace/rd_analysis_output.gz" -f 491000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/bwaves_r_base.x86_64-m64 -- bwaves_4  <bwaves_4.in &
go 503 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 727B_trace/rd_analysis_output.gz" -f 727000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/bwaves_r_base.x86_64-m64 -- bwaves_4  <bwaves_4.in &
go 503 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 267B_trace/rd_analysis_output.gz" -f 267000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/bwaves_r_base.x86_64-m64 -- bwaves_4  <bwaves_4.in &
pids[2]=$!
# MCF
go 505 
cd run/run_base_refrate_x86_64-m64.0000 && /root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 612B_trace/rd_analysis_output.gz" -f 612000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/mcf_r_base.x86_64-m64 -- inp.in    & 
go 505 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 775B_trace/rd_analysis_output.gz" -f 775000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/mcf_r_base.x86_64-m64 -- inp.in    & 
go 505 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 911B_trace/rd_analysis_output.gz" -f 911000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/mcf_r_base.x86_64-m64 -- inp.in    & 
go 505 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 505B_trace/rd_analysis_output.gz" -f 505000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/mcf_r_base.x86_64-m64 -- inp.in    & 
go 505 
cd run/run_base_refrate_x86_64-m64.0000 &&
/root/developing/test/trace/target/release/trace plugin -d -t "/root/developing/test/trace/plugins/rd_analysis/target/release/librd_analysis.so -o 130B_trace/rd_analysis_output.gz" -f 130000000000 -l 1000000000 ../run_base_refrate_x86_64-m64.0000/mcf_r_base.x86_64-m64 -- inp.in    & 
