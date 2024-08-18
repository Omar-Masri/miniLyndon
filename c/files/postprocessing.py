import sys
import re

def hamming_distance(string1, string2):
    return sum(c1 != c2 for c1, c2 in zip(string1, string1))

read_file = '/home/omarm/Desktop/UNIMIB/Stage/Stage/Codes/DeepSimulator/Ecoli_K12_DH10B_DeepSimu/sampled_read.fasta'

with open(read_file, 'r') as in_sequence:
    file_seq = in_sequence.read()

file_seq = re.findall(r'[^>]+', file_seq)
file_seq = [re.findall(r'.+', seq) for seq in file_seq]

read_seq_dict = {}
for seq in file_seq:
    read_seq_dict[seq[0]] = seq[1]

len(file_seq)

for line in sys.stdin:
	r = line.split()

	read1 = str(r[0])
	read2 = str(r[5])
	(start1, end1, start2, end2, strand) = (int(r[2]), int(r[3]), int(r[7]), int(r[8]), r[4])

	seq1 = read_seq_dict[read1][start1:end1]
	seq2 = read_seq_dict[read2][start2:end2]

	dis = hamming_distance(seq1, seq2)
	print(r[0]+"\t"+r[1]+"\t"+r[2]+"\t"+r[3]+"\t"+r[4]+"\t"+r[5]+"\t"+r[6]+"\t"+r[7]+"\t"+r[8]+"\t"+str(int(r[10])-dis)+"\t"+r[10]+"\t0")
