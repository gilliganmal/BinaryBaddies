

.PHONY: psql, pbc, pbpy 

psql:
	psql -h 127.0.0.1 -U baddie -d c2 


pbc:
	nanopb/generator/nanopb_generator  -s type:FT_POINTER implant.proto
	mv implant.pb.h C2_Implant/include/
	mv implant.pb.c C2_Implant/

pbpy:
	protoc  implant.proto --python_out=./cvnt/cvnt/  



