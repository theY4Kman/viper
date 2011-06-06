import sys
import ambuild.runner as runner

run = runner.Runner()
run.options.add_option('--enable-debug', action='store_const', const='1', dest='debug',
                       help='Enable debugging symbols')
run.options.add_option('--enable-optimize', action='store_const', const='1', dest='opt',
                       help='Enable optimization')
run.Configure(sys.path[0])
