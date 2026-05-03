import csv
import sys
import os

def csv_to_markdown_table(csv_path, md_path=None):
    """
    Convert a CSV file to a Markdown table format.
    
    Args:
        csv_path: Path to the input CSV file
        md_path: Path to the output Markdown file (optional).
                 If not provided, uses the same name with .md extension.
    """
    if md_path is None:
        md_path = os.path.splitext(csv_path)[0] + '.md'
    
    with open(csv_path, 'r', encoding='utf-8') as f:
        reader = csv.reader(f)
        rows = list(reader)
    
    if not rows:
        print("CSV file is empty.")
        return
    
    # Remove trailing empty rows (common in save files)
    while rows and all(cell.strip() == '' for cell in rows[-1]):
        rows.pop()
    
    # Build table header
    header = rows[0]
    num_cols = len(header)
    
    # Create separator line
    separator = ['---'] * num_cols
    
    # Format each row
    def format_row(row):
        # Pad row with empty strings if it has fewer columns
        padded_row = row + [''] * (num_cols - len(row))
        return '| ' + ' | '.join(padded_row[:num_cols]) + ' |'
    
    # Build the markdown table
    lines = [
        format_row(header),
        format_row(separator),
    ]
    for row in rows[1:]:
        lines.append(format_row(row))
    
    markdown_table = '\n'.join(lines)
    
    with open(md_path, 'w', encoding='utf-8') as f:
        f.write(markdown_table)
    
    print(f"Markdown table saved to: {md_path}")
    return markdown_table


if __name__ == '__main__':
    if len(sys.argv) > 1:
        csv_path = sys.argv[1]
    else:
        csv_path = 'test/adversarial_results.csv.save'
    
    if not os.path.exists(csv_path):
        print(f"Error: File not found: {csv_path}")
        sys.exit(1)
    
    md_path = sys.argv[2] if len(sys.argv) > 2 else None
    
    table = csv_to_markdown_table(csv_path, md_path)
    print("\nPreview:")
    print(table)