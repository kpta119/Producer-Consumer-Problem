# Producer-Consumer Problem Solution Using Semaphores

# Producer-Consumer Problem Solution

## Project Description
This project implements the classic producer-consumer problem using semaphores for synchronization. Producers generate items within a specified range and add them to a shared storage, while consumers remove items from the storage. The storage state is maintained in a file named `magazyn.txt`, and each producer/consumer thread logs its actions to a separate log file.

## Requirements
- C compiler (e.g., `gcc`)
- `pthread` library
- Linux/Unix operating system

## Compilation
```bash
gcc producer_consumer.c -o producer_consumer
./producer_consumer <num_producers> <num_consumers> <p1> <p2> <c1> <c2> [max_storage]
```
## Example of running
### 2 producers, 3 consumers, producers add 1-5 items, consumers take 1-5 items, storage max 30
```bash
./producer_consumer 2 3 1 5 1 5 30
```
