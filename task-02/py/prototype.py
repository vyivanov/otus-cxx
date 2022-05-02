import sys
from typing import Dict, List, Tuple, Callable

# T(1), S(1)
def ip_into_octets(ip_address:str) -> List[int]:
    ip_octets:List[str] = ip_address.split('.')
    assert len(ip_octets) == 4
    return [int(itm) for itm in ip_octets]

# T(1), S(1)
def ip_from_octets(ip_octets:List[int]) -> str:
    assert len(ip_octets) ==  4
    ip_address:str = f'{ip_octets[0]}.{ip_octets[1]}.{ip_octets[2]}.{ip_octets[3]}'
    return ip_address

# T(N), S(N)
def parse_stdin() -> Dict[int, Tuple[List[int], int]]:
    ip_map = dict()
    for line in sys.stdin:
        ip_address:str      = line.split('\t')[0]
        ip_octets:List[int] = ip_into_octets(ip_address)
        idx:int = (ip_octets[0] << 24) + \
                  (ip_octets[1] << 16) + \
                  (ip_octets[2] <<  8) + \
                  (ip_octets[3] <<  0)
        if idx in ip_map:
            __, counter = ip_map[idx]
            ip_map[idx] = ip_octets, 1 + counter
        else:
            ip_map[idx] = ip_octets, 1
    return ip_map

# T(N), S(1)
def print_stdout(ip_map:Dict[int, Tuple[List[int], int]],
                 ip_idx:List[int],
                 predicat:Callable[[List[int]], bool]):
    for idx in ip_idx:
        ip_octets, counter = ip_map[idx]
        if predicat(ip_octets):
            for __ in range(counter):
                ip_address:str = ip_from_octets(ip_octets)
                print(ip_address, file=sys.stdout, flush=True)

def main():
    ip_map = parse_stdin()
    ip_idx = [*ip_map.keys()]

    # T(N*logN), S(N)
    ip_idx.sort(reverse=True)

    print_stdout(ip_map, ip_idx, predicat = lambda ip_octets:
        ip_octets and True)

    print_stdout(ip_map, ip_idx, predicat = lambda ip_octets:
        ip_octets[0] == 1)

    print_stdout(ip_map, ip_idx, predicat = lambda ip_octets:
        ip_octets[0] == 46 and
        ip_octets[1] == 70)

    print_stdout(ip_map, ip_idx, predicat = lambda ip_octets:
        ip_octets[0] == 46 or
        ip_octets[1] == 46 or
        ip_octets[2] == 46 or
        ip_octets[3] == 46)

if __name__ == '__main__':
    assert ip_from_octets(ip_into_octets('127.0.0.1')) == '127.0.0.1'
    main()
