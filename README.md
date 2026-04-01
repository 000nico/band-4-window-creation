<!DOCTYPE html>
<html>
<head></head>
<body>

<h1>band4-overlay</h1>

<h2>The Problem</h2>

<p>
If you want to create a window in a high band (notifications, overlays, etc.)
so it stays on top of fullscreen games, Task Manager, and other system windows,
you would use the undocumented <code>CreateWindowInBand</code> function.
</p>

<p>The problem is that to create windows in high bands, Microsoft requires:</p>
<ul>
  <li>A valid certificate/signature on your executable</li>
  <li>Your program to be located in a protected path like <code>Program Files</code> or <code>System32</code></li>
  <li>A manifest with <code>uiAccess=true</code></li>
</ul>

<p>
And even then, your signature can be revoked depending on the purpose of your program.
</p>

<h2>The Solution</h2>

<p>
This library solves this by injecting shellcode into <code>explorer.exe</code>, which already
has the necessary permissions, and creates the window from within it — no certificate,
no protected path, no manifest required.
</p>

<h2>Usage</h2>

<pre><code>#include "overlay/overlay.hpp"

int main(){
    overlay::init(true);  // true = debug output
    
    // your code here
    
    overlay::exit(true);
}
</code></pre>

<blockquote>
  <strong>⚠️ Warning:</strong> Must be run as Administrator.
</blockquote>

<h2>How It Works</h2>

<ol>
  <li>Gets the <code>explorer.exe</code> PID via <code>GetShellWindow</code></li>
  <li>Allocates memory in <code>explorer.exe</code> with <code>VirtualAllocEx</code></li>
  <li>Writes the shellcode payload and a configuration struct into the allocated memory</li>
  <li>Spawns a remote thread in <code>explorer.exe</code> via <code>CreateRemoteThread</code></li>
  <li>The thread calls <code>CreateWindowInBand</code> from within <code>explorer.exe</code>'s context,
  bypassing all signature and permission requirements</li>
</ol>

</body>
</html>
