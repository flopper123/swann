# Add python image
FROM python:3.10.8-slim-buster

## Install notebook
WORKDIR /tmp

COPY ./requirements.txt /tmp/requirements.txt

RUN pip3 install -r requirements.txt

# Run notebooks
WORKDIR /swann

CMD jupyter notebook --ip 0.0.0.0 --no-browser --allow-root  --NotebookApp.token='' --NotebookApp.password=''