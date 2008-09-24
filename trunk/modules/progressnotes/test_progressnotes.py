
import e32

import progressnotes

note = progressnotes.ProgressNote( )
cancelled = [False]
def callback():
    print "User cancelled"
    cancelled[0] = True

# Callback must be given before opening dialog.
note.cancel_callback( callback )

# Open wait note
note.wait( )

note.update( 0, u"Waiting 3 seconds or user cancel" )

for x in xrange( 3 ):
    if cancelled[0]:
        break
    e32.ao_sleep(1)

# Close the note    
note.finish()
    
# Test progress note.    
maxval = 20
note.progress( maxval )
cancelled[0] = False

for x in xrange( maxval ):    
    note.update( x + 1, u"Progress %d\\%d" % (x + 1, maxval))
    e32.ao_sleep(0.1)
    if cancelled[0]: break
    
for x in xrange( 1, maxval ):    
    note.update( maxval - x, u"Progress %d\\%d" % (maxval - x, maxval ))
    e32.ao_sleep(0.1)
    if cancelled[0]: break
    
note.update( maxval + 1, u"Maxed")
e32.ao_sleep(0.5)
note.finish()



