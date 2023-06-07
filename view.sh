set -ex

RUN=$PWD/runs/run2
(
	cd ~/openlane
	python3 gui.py $RUN
)
