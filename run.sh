LOG_FILE=logs/larcmacs.log
MAX_LOG_SIZE=10000000

touch ${LOG_FILE}
LOG_SIZE=$(wc -l < ${LOG_FILE})

if [ ${LOG_SIZE} -ge ${MAX_LOG_SIZE} ]; then
  mv ${LOG_FILE} ${LOG_FILE}.bak
fi

build/bin/LARCmaCS 2>&1 | tee -a ${LOG_FILE}

