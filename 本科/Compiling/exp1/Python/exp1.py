import datetime


def merge(left, right):
    i, j = 0, 0
    result = []
    while i < len(left) and j < len(right):
        if left[i] <= right[j]:
            result.append(left[i])
            i += 1
        else:
            result.append(right[j])
            j += 1

    result += left[i:]
    result += right[j:]

    return result


def sort(lists):
    if len(lists) <= 1:
        return lists

    i = len(lists) // 2
    left = sort(lists[:i])
    right = sort(lists[i:])

    return merge(left, right)


if __name__ == "__main__":
    with open("F:\\vscodework\\Compiling\\test100000.txt", "r") as f:
        r = f.read()
        numlist = r.split()

        starttime = datetime.datetime.now()
        sort(numlist)
        endtime = datetime.datetime.now()

        print("Data Size: 100000\nTime: {}".format(endtime - starttime))
