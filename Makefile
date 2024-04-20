

.PHONY: psql, pbc, pbpy 

psql:
	psql -h 127.0.0.1 -U baddie -d c2 


pbc:
	nanopb/generator/nanopb_generator  -s type:FT_POINTER implant.proto
	mv implant.pb.h C2_Implant/
	mv implant.pb.c C2_Implant/

client_pbpy:
	protoc  client.proto --python_out=./cvnt/cvnt/ --pyi_out=./cvnt/cvnt/

implant_pbyp:
	protoc  implant.proto --python_out=./cvnt/cvnt/  --pyi_out=./cvnt/cvnt/



