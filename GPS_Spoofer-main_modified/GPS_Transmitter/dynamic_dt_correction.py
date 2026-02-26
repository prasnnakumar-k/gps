import socket
import struct

HOST = "127.0.0.1"
PORT = 7532


def main() -> None:
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print(f"Sending dynamic dt updates to {HOST}:{PORT}. Press Ctrl+C to exit.")

    try:
        while True:
            try:
                raw = input("Enter DT (seconds): ").strip()
            except EOFError:
                print("\nInput stream closed. Exiting.")
                break
            if not raw:
                continue

            try:
                dt = float(raw)
            except ValueError:
                print("Invalid numeric value. Please enter a valid float (e.g. 0.25).")
                continue

            sock.sendto(struct.pack("d", dt), (HOST, PORT))
            print(f"Sent dt={dt}")
    except KeyboardInterrupt:
        print("\nStopped by user.")
    finally:
        sock.close()


if __name__ == "__main__":
    main()
