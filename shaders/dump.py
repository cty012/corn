import sys


def main():
    if len(sys.argv) != 3:
        print("Usage: python dump.py in_file.bin out_file.txt")
        return

    in_file = sys.argv[1]
    out_file = sys.argv[2]

    try:
        with open(in_file, "rb") as f:
            data = f.read()

        with open(out_file, "w") as f:
            for i in range(0, len(data), 12):
                chunk = data[i:i+12]
                hex_bytes = [f"0x{b:02x}" for b in chunk]
                f.write(", ".join(hex_bytes) + ",\n")

    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
