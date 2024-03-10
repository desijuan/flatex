const std = @import("std");
const stdout = std.io.getStdOut().writer();

const MAX_LEVEL = 10;

fn findFirst(str: []const u8, start: usize, chr: u8) ?usize {
    for (start..str.len) |i| {
        if (str[i] == chr) return i;
    }
    return null;
}

fn findLast(str: []const u8, chr: u8) ?usize {
    for (0..str.len) |i| {
        const j = str.len - 1 - i;
        if (str[j] == chr) return j;
    }
    return null;
}

fn getOpeningBracketPos(str: []const u8, prefix: []const u8) !?usize {
    const keywordPos = std.mem.indexOf(u8, str, prefix);
    const comment = findFirst(str, 0, '%');

    if ((keywordPos != null) and ((comment == null) or (keywordPos.? < comment.?)))
        return findFirst(str, keywordPos.?, '{') orelse error.BracketNotFound;

    return null;
}

fn getPairingBracketPos(
    str: []const u8,
    start: usize,
    op_br: u8,
    cl_br: u8,
) !usize {
    if (str[start] != op_br) return error.NoBracket;

    var ctr: i32 = 1;
    for (start + 1..str.len) |i| {
        if (str[i] == op_br) {
            ctr += 1;
        } else if (str[i] == cl_br) {
            ctr -= 1;
            if (ctr == 0) return i;
        }
    }

    return error.BracketNotFound;
}

fn flatIt(
    source_path: []const u8,
    dest_file_writer: anytype,
    fbs: *std.io.FixedBufferStream([]u8),
    level: *u8,
) !void {
    const source_file = try std.fs.cwd().openFile(source_path, .{ .mode = .read_only });
    defer source_file.close();

    var buffered_reader = std.io.bufferedReader(source_file.reader());
    const source_file_reader = buffered_reader.reader();

    // Print level
    for (0..level.*) |_| try stdout.print("  ", .{}) else try stdout.print("{s}\n", .{source_path});
    level.* += 1;

    while (source_file_reader.streamUntilDelimiter(
        fbs.writer(),
        '\n',
        fbs.buffer.len,
    )) {
        const line = fbs.getWritten();
        fbs.reset();

        if (try getOpeningBracketPos(line, "\\input{") orelse
            try getOpeningBracketPos(line, "\\include{")) |op_br|
        {
            if (level.* >= MAX_LEVEL) return error.RecursionLimitExceeded;

            const cl_br = try getPairingBracketPos(line, op_br, '{', '}');

            const arg = line[op_br + 1 .. cl_br];
            const suffix = ".tex";

            const next_source_path = line[0 .. arg.len + suffix.len];
            for (0..arg.len) |i| {
                next_source_path[i] = arg[i];
            }
            for (0..suffix.len) |i| {
                next_source_path[i + arg.len] = suffix[i];
            }

            try flatIt(next_source_path, dest_file_writer, fbs, level);
        } else {
            if (try getOpeningBracketPos(line, "\\includepdf") orelse
                try getOpeningBracketPos(line, "\\includegraphics")) |op_br|
            {
                const cl_br = try getPairingBracketPos(line, op_br, '{', '}');
                const arg = line[op_br + 1 .. cl_br];

                // strip basename
                if (findLast(arg, '/')) |bar_pos| {
                    const offset = bar_pos + 1;
                    const new_line = line[0 .. line.len - offset];
                    for (op_br + 1..new_line.len) |i| {
                        new_line[i] = line[i + offset];
                    }
                    try dest_file_writer.writeAll(new_line);
                } else {
                    try dest_file_writer.writeAll(line);
                }
            } else {
                try dest_file_writer.writeAll(line);
            }
            try dest_file_writer.writeByte('\n');
        }
    } else |err| switch (err) {
        error.EndOfStream => {},
        else => return err,
    }

    level.* -= 1;
}

pub fn main() !u8 {
    var args = std.process.args();

    const bin: []const u8 = args.next().?;
    var source_path: []const u8 = undefined;
    var dest_path: []const u8 = undefined;
    (read_args: {
        source_path = args.next() orelse break :read_args error.InvalidArguments;
        dest_path = args.next() orelse break :read_args error.InvalidArguments;
        if (args.skip()) break :read_args error.InvalidArguments;
    } catch {
        std.debug.print("Usage: {s} <source> <dest>\n", .{bin});
        return 1;
    });

    const dest_file = try std.fs.cwd().createFile(dest_path, .{});
    errdefer std.fs.cwd().deleteFile(dest_path) catch {};
    defer dest_file.close();

    var buffered_writer = std.io.bufferedWriter(dest_file.writer());
    defer buffered_writer.flush() catch {};

    const dest_file_writer = buffered_writer.writer();

    var buffer: [4096]u8 = undefined;
    var fbs = std.io.fixedBufferStream(&buffer);

    var level: u8 = 0;
    flatIt(source_path, dest_file_writer, &fbs, &level) catch |err| switch (err) {
        error.BracketNotFound => {
            std.debug.print("error: Bracket Not Found\n", .{});
            return 1;
        },
        else => return err,
    };

    return 0;
}

//
// -- Tests --
//

test findFirst {
    const str = "01234567890123456789";
    try std.testing.expectEqual(@as(?usize, null), findFirst(str, 0, 'a'));
    try std.testing.expectEqual(@as(?usize, null), findFirst(str, 14, '3'));
    try std.testing.expectEqual(@as(?usize, 1), findFirst(str, 0, '1'));
    try std.testing.expectEqual(@as(?usize, 9), findFirst(str, 3, '9'));
}

test findLast {
    const str = "01234567890123456789";
    try std.testing.expectEqual(@as(?usize, null), findLast(str, 'a'));
    try std.testing.expectEqual(@as(?usize, 11), findLast(str, '1'));
    try std.testing.expectEqual(@as(?usize, 19), findLast(str, '9'));
}

test getOpeningBracketPos {
    const input_prefix = "\\input";

    const line1 = "\\input_blahblahblah";
    try std.testing.expectError(
        error.BracketNotFound,
        getOpeningBracketPos(line1, input_prefix),
    );

    const line2 = "%\\input{palabras}";
    const s2 = try getOpeningBracketPos(line2, input_prefix);
    try std.testing.expectEqual(@as(?usize, null), s2);

    inline for ([_][]const u8{
        "\\input{123456789 {} {}...}",
        "\\input{queseyo} % Acá hay un comentario",
    }) |line| {
        const p = try getOpeningBracketPos(line, input_prefix);
        try std.testing.expectEqual(@as(u8, '{'), line[p.?]);
    }

    const include_prefix = "\\include";

    const line3 = "\\include_blahblahblah";
    try std.testing.expectError(
        error.BracketNotFound,
        getOpeningBracketPos(line3, include_prefix),
    );

    const line4 = "%\\include{palabras}";
    const s4 = try getOpeningBracketPos(line4, include_prefix);
    try std.testing.expectEqual(@as(?usize, null), s4);

    inline for ([_][]const u8{
        "\\include{123456789 {} {}...}",
        "\\include{queseyo} % Acá hay un comentario",
    }) |line| {
        const p = try getOpeningBracketPos(line, include_prefix);
        try std.testing.expectEqual(@as(u8, '{'), line[p.?]);
    }
}

test getPairingBracketPos {
    const line1 = "a";
    try std.testing.expectError(
        error.NoBracket,
        getPairingBracketPos(line1, 0, '{', '}'),
    );

    const line2 = "\\input{queseyo";
    try std.testing.expectError(
        error.BracketNotFound,
        getPairingBracketPos(line2, 6, '{', '}'),
    );

    inline for ([_]struct { str: []const u8, start: usize }{
        .{ .str = "\\input{queseyo} % Acá hay un comentario", .start = 6 },
        .{ .str = "\\include{123456789 {} {}...}", .start = 8 },
    }) |t| {
        const p = try getPairingBracketPos(t.str, t.start, '{', '}');
        try std.testing.expectEqual(@as(u8, '}'), t.str[p]);
    }
}

test "Recursion Limit" {
    const n = 3;
    inline for (1..n + 1) |i| {
        const file_name = std.fmt.comptimePrint("0{d}.tex", .{i});
        const file_contents = std.fmt.comptimePrint(
            "\\input{{0{d}}}\n",
            .{@mod(i, n) + 1},
        );
        const file = try std.fs.cwd().createFile(file_name, .{});
        try file.writeAll(file_contents);
        file.close();
    }

    const dest_file_name = "a.tex";
    const dest_file = try std.fs.cwd().createFile(dest_file_name, .{});
    defer std.fs.cwd().deleteFile(dest_file_name) catch {};
    defer dest_file.close();

    var buffered_writer = std.io.bufferedWriter(dest_file.writer());
    const dest_file_writer = buffered_writer.writer();

    var buffer: [4096]u8 = undefined;
    var fbs = std.io.fixedBufferStream(&buffer);

    var level: u8 = 0;

    std.debug.print("\n", .{});
    try std.testing.expectError(
        error.RecursionLimitExceeded,
        flatIt("01.tex", dest_file_writer, &fbs, &level),
    );

    inline for (1..n + 1) |i| {
        const file_name = std.fmt.comptimePrint("0{d}.tex", .{i});
        try std.fs.cwd().deleteFile(file_name);
    }
}
