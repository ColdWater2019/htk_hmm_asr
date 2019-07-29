HCopy -A -D -C sample_5/analysis_1.conf -S sample_5/target_1.list
HInit -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm0/ -H sample_5/models/hmm_lx -l lx -L sample_5/lab/ lx
HInit -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm0/ -H sample_5/models/hmm_dd -l dd -L sample_5/lab/ dd
HInit -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm0/ -H sample_5/models/hmm_rx -l rx -L sample_5/lab/ rx 
HInit -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm0/ -H sample_5/models/hmm_sil -l sil -L sample_5/lab/ sil

HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm1 -H sample_5/hmms/hmm0/hmm_lx  -l lx  -L sample_5/lab/ lx 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm2 -H sample_5/hmms/hmm1/hmm_lx  -l lx  -L sample_5/lab/ lx 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm3 -H sample_5/hmms/hmm2/hmm_lx  -l lx  -L sample_5/lab/ lx 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm4 -H sample_5/hmms/hmm3/hmm_lx  -l lx  -L sample_5/lab/ lx 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm5 -H sample_5/hmms/hmm4/hmm_lx  -l lx  -L sample_5/lab/ lx 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm6 -H sample_5/hmms/hmm5/hmm_lx  -l lx  -L sample_5/lab/ lx 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm7 -H sample_5/hmms/hmm6/hmm_lx  -l lx  -L sample_5/lab/ lx 

HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm1 -H sample_5/hmms/hmm0/hmm_dd  -l dd  -L sample_5/lab/ dd
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm2 -H sample_5/hmms/hmm1/hmm_dd  -l dd  -L sample_5/lab/ dd 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm3 -H sample_5/hmms/hmm2/hmm_dd  -l dd  -L sample_5/lab/ dd 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm4 -H sample_5/hmms/hmm3/hmm_dd  -l dd  -L sample_5/lab/ dd 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm5 -H sample_5/hmms/hmm4/hmm_dd  -l dd  -L sample_5/lab/ dd 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm6 -H sample_5/hmms/hmm5/hmm_dd  -l dd  -L sample_5/lab/ dd 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm7 -H sample_5/hmms/hmm6/hmm_dd  -l dd  -L sample_5/lab/ dd 

HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm1 -H sample_5/hmms/hmm0/hmm_rx  -l rx  -L sample_5/lab/ rx 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm2 -H sample_5/hmms/hmm1/hmm_rx  -l rx  -L sample_5/lab/ rx 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm3 -H sample_5/hmms/hmm2/hmm_rx  -l rx  -L sample_5/lab/ rx 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm4 -H sample_5/hmms/hmm3/hmm_rx  -l rx  -L sample_5/lab/ rx 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm5 -H sample_5/hmms/hmm4/hmm_rx  -l rx  -L sample_5/lab/ rx 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm6 -H sample_5/hmms/hmm5/hmm_rx  -l rx  -L sample_5/lab/ rx 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm7 -H sample_5/hmms/hmm6/hmm_rx  -l rx  -L sample_5/lab/ rx 

HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm1 -H sample_5/hmms/hmm0/hmm_sil  -l sil  -L sample_5/lab/ sil 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm2 -H sample_5/hmms/hmm1/hmm_sil  -l sil  -L sample_5/lab/ sil 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm3 -H sample_5/hmms/hmm2/hmm_sil -l sil -L sample_5/lab/ sil 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm4 -H sample_5/hmms/hmm3/hmm_sil  -l sil  -L sample_5/lab/ sil 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm5 -H sample_5/hmms/hmm4/hmm_sil  -l sil  -L sample_5/lab/ sil 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm6 -H sample_5/hmms/hmm5/hmm_sil  -l sil  -L sample_5/lab/ sil 
HRest -A -D -T 1 -S sample_5/trainlist_1.txt -M sample_5/hmms/hmm7 -H sample_5/hmms/hmm6/hmm_sil  -l sil -L sample_5/lab/ sil 


