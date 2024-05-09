import argparse
import realizable


using_type = ["create"]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("type", choices=using_type)
    parser.add_argument("realizable", choices=realizable.realizable_type.keys())
    parser.add_argument("dir")
    parser.add_argument("name")

    args = parser.parse_args()
    if args.type == "create":
        realizable.Realizable(args.realizable, args.dir, args.name).realize()

if __name__ == '__main__':
    main()