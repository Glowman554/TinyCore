set -ex

RUN=$PWD/runs/run3
(
	cd ~/openlane
	python3 gui.py $RUN
)
