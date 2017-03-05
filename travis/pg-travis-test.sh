#!/bin/bash

set -eux

sudo apt-get update


# required packages
apt_packages="postgresql-$PGVERSION postgresql-server-dev-$PGVERSION postgresql-common python-pip python-dev build-essential"
pip_packages="testgres"

# exit code
status=0

# pg_config path
pg_ctl_path=/usr/lib/postgresql/$PGVERSION/bin/pg_ctl
initdb_path=/usr/lib/postgresql/$PGVERSION/bin/initdb
config_path=/usr/lib/postgresql/$PGVERSION/bin/pg_config


# bug: http://www.postgresql.org/message-id/20130508192711.GA9243@msgid.df7cb.de
sudo update-alternatives --remove-all postmaster.1.gz

# stop all existing instances (because of https://github.com/travis-ci/travis-cookbooks/pull/221)
sudo service postgresql stop
# ... and make sure they don't come back
echo 'exit 0' | sudo tee /etc/init.d/postgresql
sudo chmod a+x /etc/init.d/postgresql

# install required packages
sudo apt-get -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" -y install -qq $apt_packages

# create cluster 'test'
CLUSTER_PATH=$(pwd)/test_cluster
$initdb_path -D $CLUSTER_PATH -U $USER -A trust


# build pg_pathman (using CFLAGS_SL for gcov)
make USE_PGXS=1 PG_CONFIG=$config_path CFLAGS_SL="$($config_path --cflags_sl) -coverage"
sudo make install USE_PGXS=1 PG_CONFIG=$config_path

# set permission to write postgres locks
sudo chown $USER /var/run/postgresql/

# check build
status=$?
if [ $status -ne 0 ]; then exit $status; fi

# add pg_pathman to shared_preload_libraries and restart cluster 'test'
echo "shared_preload_libraries = 'pg_memorycontext'" >> $CLUSTER_PATH/postgresql.conf
echo "port = 55435" >> $CLUSTER_PATH/postgresql.conf
$pg_ctl_path -D $CLUSTER_PATH start -l postgres.log -w

# run regression tests
PGPORT=55435 PGUSER=$USER PG_CONFIG=$config_path make installcheck USE_PGXS=1 || status=$?

# show diff if it exists
if test -f regression.diffs; then cat regression.diffs; fi

exit $status
