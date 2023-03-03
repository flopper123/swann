# Add python image
FROM python:3.10.8-slim-buster


WORKDIR /tmp

# # Install notebook
COPY ./add_data/requirements.txt /tmp/requirements1.txt
COPY ./app/files/requirements.txt /tmp/requirements2.txt

RUN cd /tmp && pip3 install -r requirements1.txt && pip3 install -r requirements2.txt

WORKDIR /application


VOLUME /application/add_data
VOLUME /application/archive
VOLUME /application/app


# Run notebooks
CMD jupyter notebook --ip 0.0.0.0 --no-browser --allow-root  --NotebookApp.token='' --NotebookApp.password=''