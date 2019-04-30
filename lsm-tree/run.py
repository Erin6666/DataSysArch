from subprocess import call


actions = ['put_sorted', 'put_unsorted', 'get_sorted', 'get_unsorted', 'update_sorted', 'update_unsorted', 'throughput_unsorted', 'throughput_sorted'] 
buffer_sizes = ['100', '1000', '10000']
data_sizes = ['1000', '10000','100000'] 

## Select the action that you would like to test 
a = actions[5]
num_ops = '100'
# collect times for each data and buffer size 
for ds in data_sizes: 
    for bs in buffer_sizes: 

            f = open("data/%s/%s_%s_%s.txt"%(a,a,ds, bs), "w")
            call(["./lsm", ds, bs, num_ops,"put"], stdout=f)


