from PIL import Image

def png_to_packed_font_hex(image_path, threshold=128):
    # Load image and convert to grayscale
    img = Image.open(image_path).convert('L')
    width, height = img.size
    pixels = list(img.getdata())

    hex_output = []

    # Process pixels 8 at a time
    for i in range(0, len(pixels), 8):
        byte = 0
        for bit in range(8):
            if i + bit < len(pixels):
                # Pixel is black (value < 128) -> set bit to 1
                if pixels[i + bit] < threshold:
                    # Most Significant Bit (MSB) first: 
                    # The first pixel in the group is the leftmost bit (0x80)
                    byte |= (1 << (7 - bit))
        
        hex_output.append(f"0x{byte:02X}")

    return hex_output

# --- Execution ---
file_path = "pixil-frame-0.png" 
font_data = png_to_packed_font_hex(file_path)

# Formatting for a C-style array or Python list
print("Font Data:")
print(", ".join(font_data))