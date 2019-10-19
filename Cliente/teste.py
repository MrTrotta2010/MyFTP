class Any():
    def __eq__(self, other):
        return True

def main():

    _ = Any()

    x = 12
    if _ == x:
        print('True!')

if __name__=="__main__":
    main()